#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <memory>
/*-------------------------------------------------------------------------------
Variables globales */
    sf::Vector2f position_sourie;
    sf::Time t;
/*-------------------------------------------------------------------------------
Moteur Graphique */

    class Vertex_Tab : public sf::Drawable, public sf::Transformable
    {
        public:
            static std::vector<Vertex_Tab*> table;
            Vertex_Tab(sf::Texture a) : texture(a)
            {
            	table.push_back(this);
            }
            void clear()
            {
            	tab.clear();
            }

            void push_back(sf::Vertex a)
            {
            	tab.push_back(a);
            }

            static void clear_all()
            {
                for(auto i : table)
                {
                    i->clear();
                }
            }

        private:
            void draw(sf::RenderTarget& target, sf::RenderStates states) const
            {
                states.texture = &texture;
                target.draw(&tab[0], tab.size(), sf::Triangles, states);
            }
            sf::Texture texture;
            std::vector<sf::Vertex> tab;
    };
    std::vector<Vertex_Tab*> Vertex_Tab::table;
/*-------------------------------------------------------------------------------
Classe ID */

    class ID
    {
        friend bool operator<(ID const& a, ID const& b);
        friend bool operator==(ID const& a, ID const& b);
        friend std::hash<ID>;
        public:
            ID()
            {
            	if(libre.empty())
            	{
            		m_ID = cpt++;
            	}
            	else
            	{
            		m_ID = libre.top();
            		libre.pop();
            	}
            }

            void sup()
            {
            	libre.push(m_ID);
            }

        private:
            size_t m_ID;
            static std::stack<size_t> libre;
            static size_t cpt;
    };
    std::stack<size_t> ID::libre;
    size_t ID::cpt = 0;

    namespace std
    {
    	template<>
    	class hash<ID>
    	{
    		public:
        	size_t operator()(const ID &id ) const 
        	{
            	return id.m_ID;
        	}
    	};
	};

    bool operator<(ID const& a, ID const& b)
    {
    	return (a.m_ID < b.m_ID);
    }

    bool operator==(ID const& a, ID const& b)
    {
    	return (a.m_ID == b.m_ID);
    }
/*-------------------------------------------------------------------------------
Classe qui accumule toutes les instances de data */

    class base
    {
        public:
            base()
            {
                table.push_back(this);
            }

            static std::vector<base*> table;

            virtual void erase(ID const& a) =0;

            static void clear(ID a)
            {
            	for(auto i : table)
            	{
            	    i->erase(a);
            	}
            	a.sup();
            }
    };
    std::vector<base*> base::table;
/*-------------------------------------------------------------------------------
Classe 'data' et 'cara' qui gère les données sous forme de map et set d'ID. */

    template<typename T> 
    class data : base
    {
        private:
        	std::map<ID, T> table;
            typename std::map<ID, T>::iterator it1;
            typename std::map<ID, T>::iterator it2;
            //std::unordered_map<ID, T> table;
            //typename std::unordered_map<ID, T>::iterator it1;
            //typename std::unordered_map<ID, T>::iterator it2;

            bool verrou;
            
        public:
            T& operator[](ID const& a)
            {
                return table[a]; 
            }
            template<typename T2>
            void operator()(T2 const& a)
            {
                verrou = false;
                for (it1 = table.begin(); it1 != table.end();)
                {
                    a(it1->first);
                    if(!verrou) ++it1;
                    else verrou = false;
                }
            }
            void erase(ID const& a)
            {
                it2 = table.find(a);
                if (it2 != table.end())
                {
                    verrou=true;
                    it1 = table.erase(it2);
                }
            }
            bool exist(ID const& a)
            {
                it2 = table.find(a);
                if (it2 != table.end()) return true;
                else return false;
            }
    };

    class cara : base
    {
        private:

        	std::set<ID> table;
            std::set<ID>::iterator it1;
            std::set<ID>::iterator it2;

            //std::unordered_set<ID> table;
            //std::unordered_set<ID>::iterator it1;
            //std::unordered_set<ID>::iterator it2;

            bool verrou;
        public:
            void insert(ID const& a)
            {
                table.insert(a); 
            }
            template<typename T>
            void operator()(T const& a)
            {
                verrou = false;
                for (it1 = table.begin(); it1 != table.end();)
                {
                    a(*it1);
                    if(!verrou) ++it1;
                    else verrou = false;
                }
            }
            void erase(ID const& a)
            {
                it2 = table.find(a);
                if (it2 != table.end())
                {
                verrou=true;
                it1 = table.erase(it2);
                }
            }
            bool exist(ID const& a)
            {
                it2 = table.find(a);
                if (it2 != table.end()) return true;
                else return false;
            }
    };
/*-------------------------------------------------------------------------------
Définition des types */
    class droite // ax + by + c = 0
    {
        public: 
        // construit une droite à partir des coefs de son equation
        droite(float const& _a, float const& _b, float const& _c) : a(_a), b(_b), c(_c) {}

        // construit une droite à partir de deux points
        droite(sf::Vector2f const& u, sf::Vector2f const& v)
        {
            if(fabs(u.x-v.x) < 0.5)
            {
                a = -1;
                b = 0;
                c = u.x;
            }
            else
            {
                a = (u.y-v.y)/(u.x-v.x);
                b = -1;
                c = u.y - u.x * a;
            }
        }
        // détermine si les points u et v sont du même côté de la droite
        bool cp(sf::Vector2f const& u, sf::Vector2f const& v)
        {
            bool bo1= (a * u.x + b * u.y + c > 0), bo2 =(a * u.x + b * u.y + c > 0);
            return (bo1 && bo2) || ((!bo1) && (!bo2));
        }
        private:
        float a, b, c;
    };

    class equation
    {
        private:
        std::vector<float> a;
        unsigned int taille;
        public:
        unsigned int size() // retourne la taille de l'équation
        {
            return taille;
        }
        equation(std::vector<float> _a) : a(_a)
        {
            taille = a.size();
        }
        void p_scal(float const& scal) // multiplie tous les coefs par un scalaire
        {
            std::vector<float>::iterator it;
            for(it = a.begin(); it != a.end(); ++it)
            {
                *it *= scal;
            }
        }

        void s_eq(float const& scal, equation eq) // Ajout du multiple par un scalaire d'une autre equation
        {
            eq.p_scal(scal);
            std::vector<float>::iterator it1 = a.begin();
            std::vector<float>::iterator it2 = eq.a.begin();
            unsigned int min = std::min(taille , eq.size());
            for(int i = 0 ; i != min; ++i)
            {   
                *it1 += *it2;
                ++it1;
                ++it2;
            }
        }
    };

    class matrice
    {
        private:
        std::vector<equation> a;
        unsigned int m,n;    
        matrice(std::vector<equation> _a) : a(_a)
        {

        }
    };

    class figure
    {
        public:
            virtual bool collision(sf::Vector2f const& pt) const =0;
            virtual ~figure(){}
            virtual figure* clone() const = 0;
    };

    class triangle : public figure
    {
        private:
            sf::Vector2f s1;
            sf::Vector2f s2;
            sf::Vector2f s3;
        public:
            triangle(sf::Vector2f const& _s1, sf::Vector2f const& _s2, sf::Vector2f const& _s3) : s1(_s1),s2(_s2),s3(_s3) {}
            bool collision(sf::Vector2f const& pt) const override
            {
                droite a(s1, s2);
                droite b(s2, s3);
                droite c(s3, s1);
                return ((a.cp(s3, pt) && b.cp(s1, pt)) && c.cp(s2, pt));
            }
            virtual ~triangle(){}
            triangle* clone() const override
            {
            	return new triangle(*this);
            }
    };

    class cercle : public figure
    {
        private:
            float rayon;
            sf::Vector2f centre;
        public:
            cercle(float _rayon, sf::Vector2f const& _centre) : rayon(_rayon), centre(_centre) {}
            bool collision(sf::Vector2f const& pt) const override
            {
                return ((pt.x - centre.x)*(pt.x - centre.x)+(pt.y - centre.y)*(pt.y - centre.y) <= rayon*rayon); 
            }
        	virtual ~cercle(){}
        	cercle* clone() const override
            {
            	return new cercle(*this);
            }
    };

    class rectangle : public figure
    {
        private:
            sf::Vector2f a;
            sf::Vector2f b;
        public:
            rectangle(sf::Vector2f const& _a, sf::Vector2f const& _b) : a(_a), b(_b) {}
            bool collision(sf::Vector2f const& pt) const override
            {
                return (a.x < pt.x && pt.x < b.x && a.y < pt.y && pt.y < b.y ); 
            }
	        virtual ~rectangle(){}
	        rectangle* clone() const override
	        {
	        	return new rectangle(*this);
	        }
    };

    class forme
    {
        private:
            std::vector<figure*> table;
        public:
            void add(figure* a)
            {
                table.push_back(a);
            }

            bool collision(sf::Vector2f const& pt)
            {
                bool result=false;
                for(unsigned int i = 0; i < table.size(); i++)
                {
                    if (table[i]->collision(pt))
                    {
                        result = true;
                        i = table.size();
                    }
                }
                return result;
            }
            ~forme()
            {
                for(auto i : table)
                {
                    delete i;
                }
            }
            forme& operator=(forme const& _forme)
            {
            	if(this != & _forme)
            	{
            		for(auto i : table)
	                {
	                    delete i;
	                }
	                for(auto i : _forme.table)
	                {
              			table.push_back(i->clone());
	                }
            	}
            	return *this;
            }

    };  
/*-------------------------------------------------------------------------------
Fonctions utiles */

	//-> classe pour les fonctions aléatoires

    class Rd
    {
    public:
    static bool comp(int const a, int const b)
    {
        std::uniform_int_distribution<> dist(1,b);
        int res = dist(A);
        if(res <= a)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    static int tir(int const a, int const b)
    {
        std::uniform_int_distribution<> dist(a,b);
        return dist(A);
    }

    static sf::Color couleur()
    {
        std::uniform_int_distribution<> dist(0,255);
        sf::Color a(dist(A),dist(A),dist(A));
        return a;
    }
    private:
    static std::default_random_engine A;
    };

    std::default_random_engine Rd::A(std::chrono::high_resolution_clock::now().time_since_epoch().count());
/*-------------------------------------------------------------------------------
Instanciation des datas */
    data<Vertex_Tab*> VT;
    data<sf::Vector2f> position;
    data<sf::Vector2f> dimensions;
    data<float> orientation;
    data<sf::Vector2f> vitesse;
    data<sf::Vector2f> acceleration;
    data<sf::Color> couleur;
    data<sf::Vector2f> textureCoo;
    data<sf::Time> temps_restant;
    data<forme> boite_collision;
    cara solide;
    cara recentrer;
    cara phenix;
    cara clicable;
/*-------------------------------------------------------------------------------
Objets du jeux: */

    void create_sprite(Vertex_Tab & _VT)
    {
        ID id;
        VT[id] = & _VT;
        position[id] = sf::Vector2f(Rd::tir(0, 1600), Rd::tir(0, 1200));
        textureCoo[id] = sf::Vector2f(Rd::tir(0, 2560), Rd::tir(100, 1920));
        couleur[id] = Rd::couleur();
        dimensions[id] = sf::Vector2f(Rd::tir(5, 15), Rd::tir(5, 15));
        vitesse[id] = sf::Vector2f(Rd::tir(-300, 300), Rd::tir(-300, 300));
        recentrer.insert(id);
        phenix.insert(id);
        temps_restant[id] = sf::milliseconds(Rd::tir(200, 1000));
    }

    void bouton(Vertex_Tab & _VT)
    {
        ID id;
        VT[id] = & _VT;
        position[id] = sf::Vector2f(Rd::tir(0, 1600), Rd::tir(0, 1200));
        textureCoo[id] = sf::Vector2f(Rd::tir(0, 2560), Rd::tir(100, 1920));
        couleur[id] = Rd::couleur();
        dimensions[id] = sf::Vector2f(Rd::tir(100, 200), Rd::tir(100, 200));

        float t1 = dimensions[id].x;
        float t2 = dimensions[id].y;
        float x1 = position[id].x;
        float y1 = position[id].y;
        sf::Vector2f v1, v2;
        v1.x = x1-t1/2;
        v1.y = y1-t2/2;
        v2.x = x1+t1/2;
        v2.y = y1+t2/2;

        figure* a;
        a = new rectangle(v1,v2);
        forme b;
        b.add(a);
        boite_collision[id]=b;
        clicable.insert(id);
    }
/*-------------------------------------------------------------------------------
Fonctions qui travaillent à partir des datas */

    void load_sprite(ID const& id)
    {   
        float t1 = dimensions[id].x;
        float t2 = dimensions[id].y;
        float x1 = position[id].x;
        float y1 = position[id].y;
        float x2 = textureCoo[id].x;
        float y2 = textureCoo[id].y;
        sf::Vector2f v1, v2, v3, v4, w1, w2, w3, w4;
        sf::Vertex a, b, c, d, e, f;

        v1.x = x1-t1/2;
        v1.y = y1-t2/2;
        v2.x = x1+t1/2;
        v2.y = y1-t2/2;
        v3.x = x1+t1/2;
        v3.y = y1+t2/2;
        v4.x = x1-t1/2;
        v4.y = y1+t2/2;

        w1.x = x2-t1/2;
        w1.y = y2-t2/2;
        w2.x = x2+t1/2;
        w2.y = y2-t2/2;
        w3.x = x2+t1/2;
        w3.y = y2+t2/2;
        w4.x = x2-t1/2;
        w4.y = y2+t2/2;

        a.position = v1;
        a.texCoords = w1;
        a.color = couleur[id];
        VT[id]->push_back(a);

        b.position = v2;
        b.texCoords = w2;
        b.color = couleur[id];
        VT[id]->push_back(b);

        c.position = v4;
        c.texCoords = w4;
        c.color = couleur[id];
        VT[id]->push_back(c);

        d.position = v2;
        d.texCoords = w2;
        d.color = couleur[id];
        VT[id]->push_back(d);

        e.position = v3;
        e.texCoords = w3;
        e.color = couleur[id];
        VT[id]->push_back(e);

        f.position = v4;
        f.texCoords = w4;
        f.color = couleur[id];
        VT[id]->push_back(f);
    }

    void load_sprites()
    { 
        Vertex_Tab::clear_all();
        position(load_sprite);
    }

    void move_sprite(ID const& id)
    {
    	position[id] += vitesse[id] * t.asSeconds();
    }
    void move_sprites(){vitesse(move_sprite);}

    void f_recentrer(ID const& id)
    {
        if(position[id].x > 1600 || position[id].x < 0 || position[id].y > 1200 || position[id].y < 0)
        {
            position[id] = sf::Vector2f(800,600);
        }
    }
    void F_recentrer() {recentrer(f_recentrer);}
    

    void mourir(ID const& id)
    {
    	temps_restant[id] -= t;
    	if(temps_restant[id] < sf::seconds(0))
        {
            if(phenix.exist(id)) create_sprite(*VT[id]);
            base::clear(id);
        }
    }
    void F_mourir(){temps_restant(mourir);}	

    void est_clique(ID const& id)
    {
    	if(boite_collision[id].collision(position_sourie))
    	{
    		couleur[id] = Rd::couleur();
    	}
    }
    void F_est_clique(){clicable(est_clique);}
/*-------------------------------------------------------------------------------
Le Main */
    
    int main()
    {
        sf::Texture texture;
        texture.loadFromFile("ressources/image.jpg");
        Vertex_Tab V(texture);
        unsigned int nb_boucles=0;
        for(int i = 0; i !=50000; ++i) create_sprite(V);
        //bouton(V);

    	sf::Clock horloge;
    	sf::Clock chrono;

        sf::RenderWindow window(sf::VideoMode(1600, 1200), "test");
        window.setVerticalSyncEnabled(true);   
        
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if(event.type == sf::Event::Closed) window.close();
                if(event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        F_est_clique();
                    }
                }
            }
            position_sourie.x = (float)sf::Mouse::getPosition(window).x;
            position_sourie.y = (float)sf::Mouse::getPosition(window).y;
            t = horloge.restart();

            F_mourir();
            move_sprites();
            F_recentrer();
            load_sprites();

            window.clear();
            window.draw(V);
            window.display();

   			if(chrono.getElapsedTime() > sf::seconds(5))nb_boucles++;
            if(chrono.getElapsedTime() > sf::seconds(25)) window.close();
        }
        
        std::cout << (chrono.getElapsedTime()).asSeconds() << " secondes ecoulees." << std::endl ;
        std::cout << nb_boucles/20.0 << " images par secondes." << std::endl ;
        return 0;
    }