require 'ropework'

module QML
  class Wrapper
    include Ropework::Object

    class << self
      attr_accessor :meta_object
      private :meta_object=
    end

    attr_accessor :pointer
    private :pointer=

    def custom_data
      @custom_data ||= {}
    end

    def qml_eval(str)
      context = Context.for_object(self)
      fail QMLError, 'belongs to no context' unless context
      context.eval(self, str)
    end

    def inspect
      klass = self.class
      property_inspect = klass.instance_properties.sort
        .reject { |name| klass.instance_property(name).alias? }
        .map do |name|
          "#{name}=" +
            begin
              property(name).value.inspect
            rescue ConversionError
              "<unsupported type>"
            end
        end
        .join(' ')
      classname = klass.name || "[class for #{klass.meta_object.name}]"
      "#<#{classname}:#{__id__} #{property_inspect}>"
    end

    alias_method :to_s, :inspect

    def owned_by_ruby?
      pointer.owned?
    end

    def owned_by_ruby=(owned)
      pointer.owned = owned
    end
  end
end